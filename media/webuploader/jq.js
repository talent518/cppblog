(function($) {
	var _options = {
		pick: {
			id: '#filePicker',
			innerHTML: '选择文件',
			multiple: false
		},
		dnd: '#dndArea',
		addButton: {
			id: '#filePicker2',
			label: '继续添加',
			multiple: false
		},
		accept: {
			title: '图片',
			extensions: 'gif,jpg,jpeg,bmp,png',
			mimeTypes: 'image/*'
		},
		formData: {},
		swf: '/swf/Uploader.swf',
		server: location.href,
		change: function(file) {},
		afterChange: function(uploader, file, cur, prev) {}
	};
	$.fn.webuploader = function(options) {
		options = $.extend({}, _options, options);

		var $wrap = this,

			// 图片容器
			$queue = $('<ul class="filelist"></ul>').appendTo($wrap.find('.queueList')),

			// 状态栏，包括进度和控制按钮
			$statusBar = $wrap.find('.statusBar'),

			// 文件总体选择信息。
			$info = $statusBar.find('.info'),

			// 上传按钮
			$upload = $wrap.find('.uploadBtn'),

			// 没选择文件之前的内容。
			$placeHolder = $wrap.find('.placeholder'),

			$progress = $statusBar.find('.progress').hide(),

			// 添加的文件数量
			fileCount = 0,

			// 添加的文件总大小
			fileSize = 0,

			// 可能有pedding, ready, uploading, confirm, done.
			state = 'pedding',

			// 所有文件的进度信息，key为file id
			percentages = {},

			// WebUploader实例
			uploader;

		// 实例化
		uploader = WebUploader.create($.extend({
			pick: options.pick,
			dnd: options.dnd,
			chunked: true,
			accept: options.accept,
			duplicate: false,
			server: options.server,
			paste: $wrap.selector || $wrap,
			formData: options.formData,
			compress: false,
			resize: false
		}, options.options));

		// 添加“添加文件”的按钮，
		uploader.addButton(options.addButton);

		// 当有文件添加进来时执行，负责view的创建
		function addFile(file) {
			var $li = $('<li id="' + file.id + '">' +
				'<p class="title">' + file.name + '</p>' +
				'<p class="imgWrap"></p>' +
				'<p class="progress"><span></span></p>' +
				'</li>'),

				$btns = $('<div class="file-panel">' +
					'<span class="cancel">删除</span>' + '</div>').appendTo($li),
				$prgress = $li.find('p.progress span'),
				$wrap = $li.find('p.imgWrap'),
				$info = $('<p class="error"></p>'),

				showError = function(code) {
					switch (code) {
						case 'interrupt':
							text = '上传暂停';
							break;

						default:
							text = '上传失败，请重试';
							break;
					}

					$info.text(text).appendTo($li);
				};

			if (file.getStatus() === 'invalid') {
				showError(file.statusText);
			} else {
				$wrap.text('大小：' + file.size);
				percentages[file.id] = [file.size, 0];
				file.rotation = 0;
			}

			file.on('statuschange', function(cur, prev) {
				$wrap.text('');
				options.change.call(uploader, file, cur, prev);
				if (prev === 'progress') {
					$prgress.hide().width(0);
				} else if (prev === 'queued') {
					$li.off('mouseenter mouseleave');
					$btns.remove();
				}

				// 成功
				if (cur === 'error' || cur === 'invalid') {
					showError(file.statusText);
					percentages[file.id][1] = 1;
				} else if (cur === 'interrupt') {
					showError('interrupt');
				} else if (cur === 'queued') {
					percentages[file.id][1] = 0;
				} else if (cur === 'progress') {
					$info.remove();
					$prgress.css({
						display: 'block',
						width: '10%'
					});
				} else if (cur === 'complete') {
					$li.append('<span class="success"></span>');
				}

				$li.removeClass('state-' + prev).addClass('state-' + cur);
				options.afterChange.call(uploader, file, cur, prev);
			});

			$li.on('mouseenter', function() {
				$btns.stop().animate({
					height: 30
				});
			});

			$li.on('mouseleave', function() {
				$btns.stop().animate({
					height: 0
				});
			});

			$btns.on('click', 'span', function() {
				uploader.removeFile(file);
			});

			$li.appendTo($queue);
		}

		// 负责view的销毁
		function removeFile(file) {
			var $li = $('#' + file.id);

			delete percentages[file.id];
			updateTotalProgress();
			$li.off().find('.file-panel').off().end().remove();
		}

		function updateTotalProgress() {
			var loaded = 0,
				total = 0,
				spans = $progress.children(),
				percent;

			$.each(percentages, function(k, v) {
				total += v[0];
				loaded += v[0] * v[1];
			});

			percent = total ? loaded / total : 0;

			spans.eq(0).text(Math.round(percent * 100) + '%');
			spans.eq(1).css('width', Math.round(percent * 100) + '%');
			updateStatus();
		}

		function updateStatus() {
			var text = '',
				stats;

			if (state === 'ready') {
				text = '选中' + fileCount + '张图片，共' +
					WebUploader.formatSize(fileSize) + '。';
			} else if (state === 'confirm') {
				stats = uploader.getStats();
				if (stats.uploadFailNum) {
					text = '已成功上传' + stats.successNum + '张照片至XX相册，' +
						stats.uploadFailNum + '张照片上传失败，<a class="retry" href="#">重新上传</a>失败图片或<a class="ignore" href="#">忽略</a>'
				}

			} else {
				stats = uploader.getStats();
				text = '共' + fileCount + '张（' +
					WebUploader.formatSize(fileSize) +
					'），已上传' + stats.successNum + '张';

				if (stats.uploadFailNum) {
					text += '，失败' + stats.uploadFailNum + '张';
				}
			}

			$info.html(text);
		}

		function setState(val) {
			var file, stats;

			if (val === state) {
				return;
			}

			$upload.removeClass('state-' + state);
			$upload.addClass('state-' + val);
			state = val;

			switch (state) {
				case 'pedding':
					$placeHolder.removeClass('element-invisible');
					$queue.empty().hide();
					$statusBar.addClass('element-invisible');
					percentages = {};
					fileCount = 0;
					fileSize = 0;
					uploader.refresh();
					uploader.reset();
					break;

				case 'ready':
					$placeHolder.addClass('element-invisible');
					$(options.addButton.id).removeClass('element-invisible');
					$queue.show();
					$statusBar.removeClass('element-invisible');
					uploader.refresh();
					break;

				case 'uploading':
					$(options.addButton.id).addClass('element-invisible');
					$progress.show();
					$upload.text('暂停上传');
					break;

				case 'paused':
					$progress.show();
					$upload.text('继续上传');
					break;

				case 'confirm':
					$progress.hide();
					$upload.text('开始上传');

					stats = uploader.getStats();
					if (stats.successNum && !stats.uploadFailNum) {
						setState('finish');
						return;
					}
					break;
				case 'finish':
					stats = uploader.getStats();
					if (stats.successNum) {
					} else {
						// 没有成功的图片，重设
						// state = 'done';
					}
					break;
			}

			updateStatus();
		}
		
		uploader.setState = setState;

		uploader.onUploadProgress = function(file, percentage) {
			var $li = $('#' + file.id),
				$percent = $li.find('.progress span');

			$percent.css('width', percentage * 100 + '%');
			percentages[file.id][1] = percentage;
			updateTotalProgress();
		};

		uploader.onFileQueued = function(file) {
			fileCount++;
			fileSize += file.size;

			if (fileCount === 1) {
				$placeHolder.addClass('element-invisible');
				$statusBar.show();
			}

			addFile(file);
			setState('ready');
			updateTotalProgress();
		};

		uploader.onFileDequeued = function(file) {
			fileCount--;
			fileSize -= file.size;

			if (!fileCount) {
				setState('pedding');
			}

			removeFile(file);
			updateTotalProgress();

		};

		uploader.on('all', function(type) {
			var stats;
			switch (type) {
				case 'uploadFinished':
					setState('confirm');
					break;

				case 'startUpload':
					setState('uploading');
					break;

				case 'stopUpload':
					setState('paused');
					break;

			}
		});

		uploader.onError = function(code) {
			switch(code) {
			case 'Q_TYPE_DENIED':
				$.messageDialog('请选择有效的文件类型(' + $.map(uploader.options.accept, function(v, k) {return v.extensions;}).join(',') + ')！');
				break;
			case 'Q_EXCEED_NUM_LIMIT':
				$.messageDialog('选择的文件数超出(' + uploader.options.fileNumLimit + ')限制！');
				break;
			case 'Q_EXCEED_SIZE_LIMIT':
				$.messageDialog('文件总大小超出(' + WebUploader.formatSize(uploader.options.fileSizeLimit) + ')限制！');
				break;
			case 'F_EXCEED_SIZE':
				$.messageDialog('单文件大小超出(' + WebUploader.formatSize(uploader.options.fileSingleSizeLimit) + ')限制！');
				break;
			case 'F_DUPLICATE':
				$.messageDialog('文件重复选中！');
				break;
			default:
				$.messageDialog(code);
			}
		};

		$upload.on('click', function() {
			if (state === 'ready') {
				uploader.upload();
			} else if (state === 'paused') {
				uploader.upload();
			} else if (state === 'uploading') {
				uploader.stop();
			}
		});

		$info.on('click', '.retry', function() {
			uploader.retry();
			$info.html('');
		});

		$info.on('click', '.ignore', function() {
			$.each(uploader.getFiles('error'), function(file) {
				uploader.removeFile(file);
			});
			$info.html('');
		});

		$upload.addClass('state-' + state);
		updateTotalProgress();
		
		$.extend(uploader, {
			onUploadBeforeSend: function(object, data, headers) {
				data.md5=object.file.md5;
			},
			onUploadAccept: function(object, ret, fn) {
				object.file.statusText = ret.message;
				if(!ret.status) {
					fn('invalid');
				}
				return true;
			},
			onUploadError: function(file, reason) {
				$('#'+file.id).find('p.imgWrap').text(reason);
			}
		});
		
		return uploader;
	};

	// 扩展md5逻辑
	WebUploader.Uploader.register({
		'before-send-file': 'preupload'
	}, {
		preupload: function(file) {
			var me = this,
				owner = this.owner,
				server = me.options.server,
				deferred = WebUploader.Deferred(),
				blob = file.source.getSource(),
				$percent = $('.progress span', '#' + file.id),
				md5Complete,
				isTried = false;

			owner.md5File(file).progress(function(percentage) {
				$percent.css('width', percentage * 100 + '%');
			}).then(md5Complete = function (ret) {
				file.md5 = ret;
				$.ajax(server, {
					dataType: 'json',
					data: {
						md5: ret,
						name: file.name,
						size: file.size
					},
					timeout: 10000,
					success: function( res ) {
						if ( res.exists || !res.status ) {
							owner.skipFile( file );
							$('#'+file.id).find('p.imgWrap').text(res.message);
							if(res.status) {
								owner.trigger('uploadSuccess', file, res);
								owner.trigger('uploadComplete', file);
							}
						}
						deferred.resolve( true );
					},
					error: function(xhr, status, t) {
						if(status === 'timeout') {
							md5Complete(ret);
							return;
						}

						var reason = '网络错误';
						if(status === 'timeout') {
							reason = '秒传超时！';
						} else if(status === 'parsererror') {
							reason = '数据格式错误！';
						} else if(status === 'notmodified') {
							reason = '数据未修改(变更)！';
						}
						file.setStatus( 'error', reason );
						owner.trigger('uploadError', file, reason);
						owner.skipFile( file );
						deferred.resolve( true );
					}
				});
			});

			return deferred.promise();
		}
	});
})(jQuery);
